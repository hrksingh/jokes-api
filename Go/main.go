package main

import (
	"encoding/json"
	"fmt"
	"net/http"
	"time"

	"github.com/gin-gonic/gin"
)

const ExternalURL = "https://icanhazdadjoke.com/search"

// JokeResponse defines the structure of the external API response
type JokeResponse struct {
	Results []struct {
		Joke string `json:"joke"`
	} `json:"results"`
}

func main() {
	r := gin.Default()

	// --- Middleware (Interceptor) ---
	r.Use(func(c *gin.Context) {
		start := time.Now()
		path := c.Request.URL.Path
		method := c.Request.Method

		fmt.Printf("[START] %s %s\n", method, path)

		c.Next() // Process request

		duration := time.Since(start).Milliseconds()
		fmt.Printf("[END] %s %s - %d (%dms)\n", method, path, c.Writer.Status(), duration)
	})

	// --- Routes ---
	r.GET("/v1/api/jokes", getAllJokes)

	r.Run(":3000")
}

func getAllJokes(c *gin.Context) {
	client := &http.Client{Timeout: 10 * time.Second}

	req, _ := http.NewRequest("GET", ExternalURL, nil)
	req.Header.Set("Accept", "application/json")

	resp, err := client.Do(req)
	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"detail": "External joke service is unavailable"})
		return
	}
	defer resp.Body.Close()

	var result JokeResponse
	if err := json.NewDecoder(resp.Body).Decode(&result); err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"detail": "Error parsing jokes"})
		return
	}

	var jokes []string
	for _, item := range result.Results {
		jokes = append(jokes, item.Joke)
	}

	c.JSON(http.StatusOK, gin.H{"jokes": jokes})
}
