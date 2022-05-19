import lombok.extern.slf4j.Slf4j;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.DynamicTest;
import org.junit.jupiter.api.TestFactory;
import org.mipt.atp.db.helpers.FileHelper;
import org.mipt.atp.db.helpers.NonTestTask;
import org.mipt.atp.db.helpers.SubtestType;
import org.mipt.atp.db.shell.ShellExecutor;
import org.mipt.atp.db.shell.model.ShellExecResult;
import org.mipt.atp.db.workflow.model.Subtask;
import org.mipt.atp.db.workflow.model.Task;
import org.mipt.atp.db.workflow.model.Workflow;
import org.testcontainers.containers.Container.ExecResult;
import org.testcontainers.containers.PostgreSQLContainer;
import org.testcontainers.containers.output.Slf4jLogConsumer;
import org.testcontainers.containers.wait.strategy.Wait;
import org.testcontainers.junit.jupiter.Container;
import org.testcontainers.junit.jupiter.Testcontainers;
import org.testcontainers.utility.DockerImageName;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Optional;
import java.util.stream.Collectors;
import java.util.stream.Stream;

@Slf4j
@Testcontainers
public class RunnerTest {

    public static FileHelper helper = new FileHelper();

    public static int PORT;
    public static String USERNAME;
    public static String PASSWORD;
    public static String DATABASE_NAME;

    static {
        PORT = Integer.parseInt(helper.getRunnerProperty("org.mipt.atp.db.runner.postgres.port", "5432"));
        USERNAME = helper.getRunnerProperty("org.mipt.atp.db.runner.postgres.username", "postgres");
        PASSWORD = helper.getRunnerProperty("org.mipt.atp.db.runner.postgres.password", "postgres");
        DATABASE_NAME = helper.getRunnerProperty("org.mipt.atp.db.runner.postgres.database", "org_mipt_atp_db");
    }

    private static final List<Workflow> workflows = helper.getWorkflows();

    @Container
    public PostgreSQLContainer<?> postgre = new PostgreSQLContainer<>(DockerImageName.parse("postgres"))
            .withUsername(USERNAME)
            .withPassword(PASSWORD)
            .withDatabaseName(DATABASE_NAME)
            .withExposedPorts(PORT, PORT)
            .withEnv("PGPASSWORD", PASSWORD)
            .withLogConsumer(new Slf4jLogConsumer(log))
            .waitingFor(Wait.forLogMessage(".*database system is ready to accept connections.*", 2));

    @TestFactory
    public Stream<DynamicTest> test() {
        System.out.println("JDBC URL for container: " + postgre.getJdbcUrl());
        return new ArrayList<DynamicTest>() {{
            for (Workflow workflow: workflows) {
                add(DynamicTest.dynamicTest(
                        "Resolving: " + workflow.getComment(),
                        () -> testWorkflow(workflow)
                ));
            }
        }}.stream();
    }

    public void testWorkflow(Workflow workflow) throws IOException {
        System.out.format("RUNNING WORKFLOW about: %s\n\n", workflow.getComment());

        testNonTestWorkflow(NonTestTask.BEFORE_ALL, workflow.getBeforeAll());

        for (Task task: workflow.getTasks()) {
            System.out.format(">>> RUNNING [MAIN] task about: %s\n\n", task.getComment());

            for (Subtask subtask : task.getSubtasks()) {
                testNonTestWorkflow(NonTestTask.BEFORE_EACH, workflow.getBeforeEach());
                processExecResults(getExecResults(subtask));
                testNonTestWorkflow(NonTestTask.AFTER_EACH, workflow.getAfterEach());
            }
        }

        testNonTestWorkflow(NonTestTask.AFTER_ALL, workflow.getAfterAll());
    }

    public void testNonTestWorkflow(NonTestTask nonTestTask, List<Task> tasks) throws IOException {
        for (Task task: tasks) {
            System.out.format(">>> RUNNING [%s] task about: %s\n\n", nonTestTask.name(), task.getComment());

            for (Subtask subtask : task.getSubtasks()) {
                getExecResults(subtask);
            }
        }
    }

    public List<Optional<?>> getExecResults(Subtask subtask) throws IOException {
        return switch (SubtestType.from(subtask.getType())) {
            case SIMPLE_SQL  -> executeSimpleSql(helper.readSql(subtask.getFileLocation()));
            case COMPLEX_SQL -> executeComplexSql(helper.readSql(subtask.getFileLocation()));
            case SHELL       -> executeShell(helper.getShellScriptPath(subtask.getFileLocation()));
        };
    }

    public void processExecResults(List<Optional<?>> execResults) {
        for (var execResult: execResults) {
            if (execResult.isPresent() && execResult.get() instanceof ExecResult res) {
                System.out.println(res.getStdout());
                System.err.println(res.getStderr());
                Assertions.assertEquals(0, res.getExitCode());
            } else if (execResult.isPresent() && execResult.get() instanceof ShellExecResult res) {
                System.out.println(res.stdout());
                System.err.println(res.stderr());
                Assertions.assertEquals(0, res.exitCode());
            } else {
                System.err.println("No result :(");
                Assertions.fail();
            }
        }
    }

    protected List<Optional<?>> executeShell(String path) {
        return List.of(Optional.of(new ShellExecutor().exec(path)));
    }

    protected List<Optional<?>> executeComplexSql(String sql) {
        return List.of(executeSql(sql));
    }

    protected List<Optional<?>> executeSimpleSql(String sql) {
        // TODO: better to use calcite to separate first-level independent sql queries
        return Arrays.stream(sql.split(";"))
                .map(this::executeSql)
                .collect(Collectors.toList());
    }

    protected Optional<?> executeSql(String sql) {
        try {
            return Optional.of(executeSqlStmt(sql));
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
        return Optional.empty();
    }

    protected ExecResult executeSqlStmt(String sql) throws IOException, InterruptedException {
        String userName = String.format("--username=%s", USERNAME);
        String dbName = String.format("--dbname=%s", DATABASE_NAME);
        String port = String.format("--port=%d", PORT);
        String command = String.format("--command=%s", sql);
        return postgre.execInContainer("psql",
                userName,
                dbName,
                port,
                command,
                "--echo-queries",
                "--quiet"
        );
    }
}
