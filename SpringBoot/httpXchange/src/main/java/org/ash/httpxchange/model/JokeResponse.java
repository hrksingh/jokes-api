package org.ash.httpxchange.model;

import com.fasterxml.jackson.annotation.JsonProperty;

import java.util.List;

public record JokeResponse(
        @JsonProperty("current_page")
        int currentPage,

        int limit,

        @JsonProperty("next_page")
        int nextPage,

        @JsonProperty("previous_page")
        int previousPage,

        List<JokeRecord> results,

        @JsonProperty("search_term")
        String searchTerm,

        int status,

        @JsonProperty("total_jokes")
        int totalJokes,

        @JsonProperty("total_pages")
        int totalPages
) {}