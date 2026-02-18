import time
from fastapi import FastAPI, HTTPException, Request
import httpx

app = FastAPI()

EXTERNAL_URL = "https://icanhazdadjoke.com/search"

# --- Interceptor (Middleware) ---
@app.middleware("http")
async def log_requests(request: Request, call_next):
    start_time = time.time()
    print(f"[START] {request.method} {request.url.path}")

    response = await call_next(request)

    process_time = (time.time() - start_time) * 1000  # Convert to ms
    print(f"[END] {request.method} {request.url.path} - {response.status_code} ({process_time:.2f}ms)")

    return response
# -------------------------------

@app.get("/v1/api/jokes")
async def get_all_jokes():
    headers = {"Accept": "application/json"}
    async with httpx.AsyncClient() as client:
        try:
            response = await client.get(EXTERNAL_URL, headers=headers)
            response.raise_for_status()
            data = response.json()
            all_jokes = [item["joke"] for item in data.get("results", [])]
            return {"jokes": all_jokes}
        except httpx.HTTPError:
            raise HTTPException(status_code=500, detail="External joke service is unavailable")

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=3000)
