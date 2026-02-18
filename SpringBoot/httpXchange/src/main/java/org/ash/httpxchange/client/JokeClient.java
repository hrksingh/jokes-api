package org.ash.httpxchange.client;

import org.ash.httpxchange.model.JokeResponse;
import org.springframework.web.service.annotation.GetExchange;
import org.springframework.web.service.annotation.HttpExchange;

@HttpExchange
public interface JokeClient {
    @GetExchange("/search")
    JokeResponse fetchJokes();
}
