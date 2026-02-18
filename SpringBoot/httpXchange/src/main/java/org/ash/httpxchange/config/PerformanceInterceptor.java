package org.ash.httpxchange.config;

import org.jspecify.annotations.NonNull;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.http.HttpRequest;
import org.springframework.http.client.ClientHttpRequestExecution;
import org.springframework.http.client.ClientHttpRequestInterceptor;
import org.springframework.http.client.ClientHttpResponse;

import java.io.IOException;

public class PerformanceInterceptor implements ClientHttpRequestInterceptor {
    private static final Logger log = LoggerFactory.getLogger(PerformanceInterceptor.class);

    @Override
    public ClientHttpResponse intercept(@NonNull HttpRequest request, byte @NonNull [] body, ClientHttpRequestExecution execution) throws IOException {
        long start = System.currentTimeMillis();

        // Execute the actual HTTP call
        ClientHttpResponse response = execution.execute(request, body);

        long duration = System.currentTimeMillis() - start;

        log.info("External API Call: [{} {}] took {}ms with status {}",
                request.getMethod(), request.getURI(), duration, response.getStatusCode());

        return response;
    }
}