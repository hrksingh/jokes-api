import Fastify from "fastify";

const fastify = Fastify();
const EXTERNAL_URL = "https://icanhazdadjoke.com/search";

// --- Interceptor (Hook) ---
fastify.addHook("onRequest", (request, reply, done) => {
    request.startTime = performance.now(); // Record start time
    console.log(`[START] ${request.method} ${request.url}`);
    done();
});

fastify.addHook("onResponse", (request, reply, done) => {
    const duration = (performance.now() - request.startTime).toFixed(2);
    console.log(
        `[END] ${request.method} ${request.url} - ${reply.statusCode} (${duration}ms)`,
    );
    done();
});
// --------------------------

fastify.get("/v1/api/jokes", async (request, reply) => {
    try {
        const response = await fetch(EXTERNAL_URL, {
            headers: { Accept: "application/json" },
        });

        if (!response.ok) throw new Error();

        const data = await response.json();
        return { jokes: data.results.map((item) => item.joke) };
    } catch (err) {
        reply
            .status(500)
            .send({ detail: "External joke service is unavailable" });
    }
});

fastify.listen({ port: 3000, host: "0.0.0.0" }, (err, address) => {
    if (err) {
        console.error(err);
        process.exit(1);
    }
    console.log(`Node server running at ${address}`);
});
