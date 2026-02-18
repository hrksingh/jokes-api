package org.ash.httpxchange.controller;

import org.ash.httpxchange.model.JokeRecord;
import org.ash.httpxchange.service.JokeService;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

@RestController
@RequestMapping("/v1/api")
public class JokeController {
    private final JokeService jokeService;

    public JokeController(JokeService jokeService) {
        this.jokeService = jokeService;
    }

    @GetMapping("/joke")
    public List<JokeRecord> getJokes() {
        return jokeService.getJokes();
    }
}
