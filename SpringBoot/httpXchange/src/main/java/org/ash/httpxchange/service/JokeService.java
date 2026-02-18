package org.ash.httpxchange.service;

import org.ash.httpxchange.client.JokeClient;
import org.ash.httpxchange.model.JokeRecord;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class JokeService {

    private final JokeClient jokeClient;

    public JokeService(JokeClient jokeClient) {
        this.jokeClient = jokeClient;
    }

    public List<JokeRecord> getJokes() {
        return jokeClient.fetchJokes().results();
    }
}