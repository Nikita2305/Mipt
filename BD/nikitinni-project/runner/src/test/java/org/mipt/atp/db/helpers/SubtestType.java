package org.mipt.atp.db.helpers;

import lombok.RequiredArgsConstructor;

import java.util.Arrays;

@RequiredArgsConstructor
public enum SubtestType {
    SIMPLE_SQL("simple_sql"),
    COMPLEX_SQL("complex_sql"),
    SHELL("shell");

    public final String type;

    public static SubtestType from(String type) {
        return Arrays.stream(SubtestType.values())
                .filter(subtestType -> subtestType.type.equals(type))
                .findFirst()
                .orElseThrow(() -> new RuntimeException("Unknown type: " + type));
    }
}
