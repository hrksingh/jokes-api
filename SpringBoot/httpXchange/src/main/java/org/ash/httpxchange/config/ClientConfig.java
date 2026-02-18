package org.ash.httpxchange.config;

import org.ash.httpxchange.client.JokeClient;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.web.client.RestClient;
import org.springframework.web.client.support.RestClientAdapter;
import org.springframework.web.service.invoker.HttpServiceProxyFactory;

@Configuration
public class ClientConfig {

    @Bean
    public JokeClient jokeClient() {
        RestClient restClient = RestClient.builder()
                .baseUrl("https://icanhazdadjoke.com")
                .requestInterceptor(new PerformanceInterceptor())
                .defaultHeader("Accept", "application/json")
                .build();

        // Create a proxy factory based on the RestClient
        HttpServiceProxyFactory factory = HttpServiceProxyFactory
                .builderFor(RestClientAdapter.create(restClient))
                .build();

        return factory.createClient(JokeClient.class);
    }
}