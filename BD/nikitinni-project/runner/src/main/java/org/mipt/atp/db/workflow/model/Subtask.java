package org.mipt.atp.db.workflow.model;

import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.Data;

@Data
public class Subtask {
    @JsonProperty("type")
    private String type;
    @JsonProperty("file_location")
    private String fileLocation;
}
