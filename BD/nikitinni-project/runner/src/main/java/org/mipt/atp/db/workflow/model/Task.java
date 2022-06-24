package org.mipt.atp.db.workflow.model;

import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.Data;

import java.util.List;

@Data
public class Task {
    @JsonProperty("comment")
    private String comment = "";
    @JsonProperty("subtasks")
    private List<Subtask> subtasks;
}
