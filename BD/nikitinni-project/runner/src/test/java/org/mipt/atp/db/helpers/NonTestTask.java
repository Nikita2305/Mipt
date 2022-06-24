package org.mipt.atp.db.helpers;

import lombok.RequiredArgsConstructor;

@RequiredArgsConstructor
public enum NonTestTask {
    BEFORE_ALL,
    BEFORE_EACH,
    AFTER_ALL,
    AFTER_EACH;
}