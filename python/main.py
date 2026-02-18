from fastapi import FastAPI, HTTPException
import httpx

app = FastAPI()

EXTERNAL_URL = "https://icanhazdadjoke.com/search"

@app.get("/v1/api/jokes")
async def get_all_jokes():
    headers = {"Accept": "application/json"}
    
    async with httpx.AsyncClient() as client:
        try:
            response = await client.get(EXTERNAL_URL, headers=headers)
            response.raise_for_status()
            data = response.json()
            
            # Extracting only the 'joke' text from each result
            all_jokes = [item["joke"] for item in data.get("results", [])]
            
            return {"jokes": all_jokes}
            
        except httpx.HTTPError as exc:
            raise HTTPException(status_code=500, detail="External joke service is unavailable")

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=3000)
