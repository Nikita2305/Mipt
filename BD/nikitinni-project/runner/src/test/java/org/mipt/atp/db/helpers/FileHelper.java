package org.mipt.atp.db.helpers;

import org.mipt.atp.db.workflow.WorkflowMapper;
import org.mipt.atp.db.workflow.model.Workflow;
import org.yaml.snakeyaml.Yaml;

import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Properties;

@SuppressWarnings("unchecked")
public class FileHelper {

    private final Path workflowsPath;

    public FileHelper() {
        String workflowsRelPath = this.getRunnerProperty("org.mipt.atp.db.runner.workflows", "workflows.yaml");
        String resourcePath = Objects.requireNonNull(this.getClass().getClassLoader().getResource(".")).getFile();
        workflowsPath = Paths.get(resourcePath, workflowsRelPath).normalize();
    }

    public String getRunnerProperty(String key, String defaultValue) {
        Properties props = new Properties();
        InputStream inputStream = this.getClass()
                .getClassLoader()
                .getResourceAsStream("runner.properties");
        try {
            props.load(inputStream);
            return props.getProperty(key, defaultValue);
        } catch (Exception ignored) {
            return defaultValue;
        }
    }

    public List<Workflow> getWorkflows() {
        try {
            InputStream inputStream = new DataInputStream(new FileInputStream(workflowsPath.toFile()));

            Yaml yaml = new Yaml();
            List<Workflow> workflows = new LinkedList<>();
            yaml.loadAll(inputStream)
                    .forEach(it ->
                            workflows.add(new WorkflowMapper()
                                    .fromMap((Map<String, Object>) it)
                            )
                    );

            return workflows;
        } catch (Exception ignored) {
            return List.of();
        }
    }

    public String getShellScriptPath(String relativePath) throws IOException {
        return Path
                .of(
                        workflowsPath.getParent().toString(),
                        relativePath
                )
                .normalize()
                .toString();
    }

    public String readSql(String relativePath) throws IOException {
        return Files.readString(
                Path.of(
                        workflowsPath.getParent().toString(),
                        relativePath
                ).normalize()
        );
    }
}
