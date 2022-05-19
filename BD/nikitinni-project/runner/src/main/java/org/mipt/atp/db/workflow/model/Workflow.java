package org.mipt.atp.db.workflow.model;

import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.Data;

import java.util.List;

@Data
public class Workflow {
    @JsonProperty("comment")
    private String comment = "";
    @JsonProperty("before_all")
    private List<Task> beforeAll = List.of();
    @JsonProperty("before_each")
    private List<Task> beforeEach = List.of();
    @JsonProperty("main_tasks")
    private List<Task> tasks;
    @JsonProperty("after_all")
    private List<Task> afterAll = List.of();
    @JsonProperty("after_each")
    private List<Task> afterEach = List.of();
}
