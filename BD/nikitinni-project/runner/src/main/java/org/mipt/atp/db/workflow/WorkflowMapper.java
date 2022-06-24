package org.mipt.atp.db.workflow;

import com.fasterxml.jackson.databind.DeserializationFeature;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.PropertyNamingStrategies;
import com.fasterxml.jackson.dataformat.yaml.YAMLFactory;
import org.mipt.atp.db.workflow.model.Workflow;

import java.io.IOException;
import java.nio.file.Paths;
import java.util.Map;

public class WorkflowMapper {
    private final ObjectMapper mapper = new ObjectMapper(new YAMLFactory())
            .setPropertyNamingStrategy(PropertyNamingStrategies.SNAKE_CASE)
            .configure(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES, true);

    public Workflow fromMap(Map<String, Object> map) {
        return mapper.convertValue(map, Workflow.class);
    }
}
