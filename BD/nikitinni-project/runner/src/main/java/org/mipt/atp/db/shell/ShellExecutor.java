package org.mipt.atp.db.shell;

import org.mipt.atp.db.shell.model.ShellExecResult;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class ShellExecutor {

    public ShellExecResult exec(String path) {
        if (path.contains("python")) {
            int a;
        }
        StringBuilder output = new StringBuilder();
        StringBuilder err = new StringBuilder();
        int exitVal;
        ProcessBuilder processBuilder = new ProcessBuilder();
        processBuilder.command("bash", path);

        try {
            Process process = processBuilder.start();
            BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
            BufferedReader errReader = new BufferedReader(new InputStreamReader(process.getErrorStream()));

            String line;
            while ((line = reader.readLine()) != null) {
                output.append(line).append("\n");
            }

            String errLine;
            while ((errLine = errReader.readLine()) != null) {
                err.append(errLine).append("\n");
            }

            exitVal = process.waitFor();

            return new ShellExecResult(exitVal, output.toString(), err.toString());
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }

        return new ShellExecResult(1, "", "Unable to exec.");
    }
}
