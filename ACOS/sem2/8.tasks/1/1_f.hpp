#include <iostream>
#include <string>
#include <vector>

extern "C" {
#include <dlfcn.h>
}

struct ClassImpl {
  public:
    ClassImpl(void (*builder)(void*));
    void buildHere(void*) const;

  private:
    void (*builder_)(void*);
};

ClassImpl::ClassImpl(void (*builder)(void*)) : builder_(builder)
{
}

void ClassImpl::buildHere(void* place) const
{
    builder_(place);
}

class AbstractClass
{
  public:
    AbstractClass();
    AbstractClass(void (*builder)(void*));
    ~AbstractClass();

  protected:
    void* newInstanceWithSize(size_t sizeofClass);
    struct ClassImpl* pImpl{nullptr};
};

AbstractClass::AbstractClass(void (*builder)(void*))
    : pImpl(new ClassImpl(builder))
{
}

AbstractClass::~AbstractClass()
{
    if (pImpl)
        delete pImpl;
}

void* AbstractClass::newInstanceWithSize(size_t sizeofClass)
{
    void* place = (void*)new (std::align_val_t(16)) char[sizeofClass];
    pImpl->buildHere(place);
    return place;
}

enum class ClassLoaderError {
    NoError = 0,
    FileNotFound,
    LibraryLoadError,
    NoClassInLibrary
};

struct ClassLoaderImpl {
  public:
    ClassLoaderImpl();
    AbstractClass* loadClass(const std::string& fullyQualifiedName);
    ClassLoaderError lastError() const;
    ~ClassLoaderImpl();

  private:
    std::vector<std::string>
    getUnits(const std::string& string, const std::string& delimiter) const;
    ClassLoaderError last_error_{ClassLoaderError::NoError};
};

ClassLoaderImpl::ClassLoaderImpl()
{
}

std::vector<std::string> ClassLoaderImpl::getUnits(
    const std::string& s,
    const std::string& delimiter) const
{
    int pos_start = 0, pos_end;
    int delim_len = delimiter.length();
    std::vector<std::string> res;
    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        std::string token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }
    res.push_back(s.substr(pos_start));
    return res;
}

AbstractClass* ClassLoaderImpl::loadClass(const std::string& fullyQualifiedName)
{
    std::vector<std::string> units = getUnits(fullyQualifiedName, "::");
    std::string lib = std::string(getenv("CLASSPATH"));
    for (const std::string& unit : units) {
        lib += "/" + unit;
    }
    lib += ".so";

    void* handle = (void*)dlopen(lib.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (handle == NULL) {
        last_error_ = ClassLoaderError::LibraryLoadError;
        return nullptr;
    }

    for (int id = 2; id >= 1; id--) {
        std::string name = "_ZN";
        for (const std::string& unit : units) {
            name += std::to_string(unit.size()) + unit;
        }
        name += "C" + std::to_string(id) + "Ev";

        auto func = (void (*)(void*))dlsym(handle, name.c_str());
        if (func != NULL) {
            return new AbstractClass(func);
        }
    }

    last_error_ = ClassLoaderError::NoClassInLibrary;
    return nullptr;
}

ClassLoaderError ClassLoaderImpl::lastError() const
{
    return last_error_;
}

ClassLoaderImpl::~ClassLoaderImpl()
{
}

class ClassLoader
{
  public:
    ClassLoader();
    AbstractClass* loadClass(const std::string& fullyQualifiedName);
    ClassLoaderError lastError() const;
    ~ClassLoader();

  private:
    struct ClassLoaderImpl* pImpl;
};

ClassLoader::ClassLoader()
{
    pImpl = new ClassLoaderImpl();
}

AbstractClass* ClassLoader::loadClass(const std::string& fullyQualifiedName)
{
    return pImpl->loadClass(fullyQualifiedName);
}

ClassLoaderError ClassLoader::lastError() const
{
    return pImpl->lastError();
}

ClassLoader::~ClassLoader()
{
    delete pImpl;
}
