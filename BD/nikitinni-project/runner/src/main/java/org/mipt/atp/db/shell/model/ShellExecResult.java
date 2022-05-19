package org.mipt.atp.db.shell.model;

public record ShellExecResult(int exitCode, String stdout, String stderr) {
}
