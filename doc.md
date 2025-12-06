# DatsNewWay / DatsSnake – Game Specification (Final)

**Source**: Consolidated English documentation based primarily on the original `doc.pdf` and the most faithful markdown renditions (`docs_1.md`, `docs_2.md`, `docs_3.md`).

---

## Overview

**DatsNewWay** (aka **DatsSnake**) is a competitive, turn‑based 3D snake game. Players control several snakes on a discrete 3D grid, collecting mandarins (food) to grow and score points while avoiding collisions with walls, obstacles, and other snakes.

---

## Servers

- **Game server**: `https://games.datsteam.dev`
- **OpenAPI / Swagger**: `https://games.datsteam.dev/openapi/3338e4e6-f9d0-4f18-86ec-c72fcd3a8c17`

All requests must include your personal authentication token:

- **Header**: `X-Auth-Token: <your_token>`

---

## Rounds

- The game consists of multiple **rounds**.
- The **schedule of rounds** can be obtained via the game API (see Swagger for endpoints).
- During a round, the **value of mandarins increases stepwise**: every **1/10** of the round duration there is a global price increase.

---

## Map

- The game world is a discrete **3D grid** in coordinates \((x, y, z)\).
- **Minimum coordinate**: `[0, 0, 0]`.
- **Maximum coordinates** along each axis (map dimensions) are returned in the API.
- **Constraints**:
  - Snakes cannot move beyond map bounds.
  - Snakes cannot move into negative coordinates.

> Intuitively: imagine a finite 3D grid with X, Y, and Z axes, where all coordinates are non‑negative and snakes move one cell at a time.

---

## Tick‑Based Mechanics

The game is **turn‑based** and proceeds in discrete **ticks**.

- **Nominal tick duration**: about **1 second** (this may be changed by the organizers).

**Order of events in each tick:**

1. All snake movements are applied (based on the current commands).
2. For every cell, if a snake head and any other object occupy the same cell, a potential collision is registered.
3. All collisions from step 2 are resolved (snake vs snake, snake vs obstacle, snake vs wall, etc.).
4. The tick ends and the new world state becomes visible.

All player commands submitted during a tick are considered for that tick; only the **latest valid** command per snake is applied.

---

## Snakes

### General

- A snake is an ordered list of pixels (cells) – its **body**.
- Length can be from **1** to infinity.
- Each player starts the game with **3 snakes**, each of length **1**.

### Movement

- When a snake **spawns or respawns**, its speed is **0** (stationary) until you give it a direction.
- Once a **direction** is set for a snake **for the first time**, it starts moving and **never stops**.
- **Speed**: always **1 pixel per tick**.
- Movement is always controlled from the **head**.

The direction is a 3D vector `[dx, dy, dz]` indicating movement by 1 cell along one axis per tick.

- Example: `[1, 0, 0]` – move along X **away from** 0.
- Example: `[0, 0, -1]` – move along Z **toward** 0.

#### Direction options (non‑reversal rule)

For snakes of length **2 or more**, there are **5 valid movement options** each tick:

- Continue forward (same direction as previous tick).
- Turn to any of the **4 perpendicular** directions in 3D space.
- A direct reversal into the cell occupied by the second segment (the "neck") is **not allowed**.

The **body‑following rule**:

- As the head moves, each body segment moves into the cell previously occupied by the segment in front of it.
- Over time, the entire body follows the exact path taken by the head.

### Growth

Snakes grow by eating mandarins.

- When a snake’s **head** moves onto a cell containing a mandarin, the snake grows.
- Growth is implemented by adding **one pixel to the tail**.
- This applies to **all mandarin types** (normal, golden, suspicious/dubious): they always increase length by 1 even if the points gained are negative.

### Visibility (Vision / Field of View)

The map is divided into invisible **30 × 30 × 30** pixel sectors.

- The player **sees**:
  - Their own snakes.
  - Other players’ snakes.
  - Obstacles.
  - Mandarins.
- The visible area is:
  - The sector(s) containing your own snakes, **plus all adjacent sectors**, including diagonals (26 neighbors around the current sector).

---

## Controls (API)

Snakes are controlled entirely via HTTP requests to the game API.

- **Primary move endpoint** (see Swagger for the exact path): typically `POST /api/move`.

**Required headers:**

- `X-Auth-Token: <your_token>`
- `Content-Type: application/json`
- Optionally for compression:
  - `Accept-Encoding: gzip, deflate`

### Command format

The key parameter is `direction`, a vector `[x, y, z]` describing the next step for a snake.

| Direction  | Effect                                             |
|-----------|-----------------------------------------------------|
| `[1, 0, 0]`  | Move along X axis (away from 0)                  |
| `[-1, 0, 0]` | Move along X axis (toward 0)                     |
| `[0, 1, 0]`  | Move along Y axis (away from 0)                  |
| `[0, -1, 0]` | Move along Y axis (toward 0)                     |
| `[0, 0, 1]`  | Move along Z axis (away from 0)                  |
| `[0, 0, -1]` | Move along Z axis (toward 0)                     |

**Important details:**

- Commands are sent as a list of snake updates:

```json
{
  "snakes": [
    {
      "id": "<snake_id>",
      "direction": [dx, dy, dz]
    }
  ]
}
```

- To **only fetch state** without changing directions, send an empty list:

```json
{
  "snakes": []
}
```

- If a snake’s command in the current request is **invalid** (e.g., invalid vector or breaking reversal rules), that specific command is **ignored**, but valid commands for other snakes in the same request are still processed.
- You may send **multiple requests in the same tick**. The **last valid command** for each snake in that tick overrides any previously sent commands before the tick is processed.

---

## Collisions

### Destruction conditions

A snake is **destroyed completely** if its **head** collides with **anything except a mandarin**, including:

- Other snakes’ heads or bodies.
- Map edges / walls.
- Obstacles.

**Special rule (body hit):**

- If an opponent’s **head** hits **your snake’s body**, the **opponent** is destroyed and **your snake survives**.

Destroyed snakes **immediately disappear** from the map.

### Respawn

After destruction a snake will respawn after a delay.

- The delay is provided in the API in the `reviveRemainMs` field.
- Respawn properties:
  - New random spawn location.
  - Speed = **0** (stationary until given a direction).
  - Length = **1**.
- Each destruction applies a **penalty** of **−10% of the current score**.

---

## Mandarins (Food)

Mandarins are the main scoring and growth resource in the game.

- They occupy **1 cell** each.
- They spawn **randomly** throughout the map during the round.

### Consumption

- When eaten by a snake (head enters their cell), mandarins **disappear**.
- If **multiple snakes** eat the **same mandarin in the same tick**, the points from that mandarin are **split equally** between them.

### Scoring principles

- The closer a mandarin is to the **center of the map**, the **more points** it is worth.
- During a round, mandarin values **increase** at discrete times:
  - Every **1/10** of the round duration, global mandarin prices increase.
- For normal mandarins, the **base price is fixed at spawn time** and then scaled by global rules as described.

### Types of mandarins

1. **Normal mandarins**
   - Give a **fixed number of points** based on position and round phase.
   - Price is determined at spawn; subsequent global rule changes apply in the same way as for normal food in the specification.

2. **Golden mandarins**
   - Worth **10×** the value of a normal mandarin at the same location and time.
   - As global mandarin prices increase over the round, the prices of **all remaining golden mandarins on the map also increase**.
   - **All golden mandarins are generated only at the beginning of the round**; no additional golden mandarins appear later.

3. **Suspicious / Dubious mandarins**
   - Their effect is **random** within a range:
     - The score change is `rand(-5, 5)` times the base cost, i.e. they may **add or subtract** points.
   - Regardless of the sign of the score, they **always grow the snake by 1**.

---

## API Response – Object Fields

The exact JSON schemas are in the OpenAPI spec, but the key fields from the original rules are:

### Your snakes

- `id`: unique snake identifier (persists across respawns).
- `direction`: direction specified for the **current** request (`[dx, dy, dz]`).
- `oldDirection`: direction used in the **previous** tick.
- `geometry`: ordered list of body pixels; the **first element is the head**.
- `deathCount`: number of times this snake has been destroyed.
- `status`: either `"alive"` or `"dead"`.
- `reviveRemainMs`: milliseconds remaining until respawn (if the snake is currently dead).

### Opponent snakes

- `geometry`: same format as for your own snakes; first element is the head.
- `status`: `"alive"` or `"dead"`.
- `kills`: number of times this snake has destroyed others by having them collide with its body.

### Mandarins

- `c`: coordinate of the mandarin, usually `[x, y, z]`.
- `points`: number of points the mandarin is worth **if eaten in that tick**.

### Errors

- If a request contains invalid parts (for example, invalid snake IDs or directions), the server may return a **list of errors** in the response describing the issues, while still applying all valid parts of the request.

---

## Networking Examples

### cURL

```bash
curl -X POST \
    -H "X-Auth-Token: YOUR_TOKEN" \
    -H "Content-Type: application/json" \
    -d '{"snakes":[]}' \
    https://games.datsteam.dev/api/move \
    -o example_response.json
```

### Python (`requests`)

```python
import requests

token = 'YOUR_TOKEN'
server_url = 'https://games.datsteam.dev'
url = f"{server_url}/api/move"

payload = {
    'snakes': []
}

headers = {
    'X-Auth-Token': token,
    'Content-Type': 'application/json',
}

response = requests.post(url, headers=headers, json=payload)

with open('example_response.json', 'w') as f:
    f.write(response.text)
```

### Compression (optional)

Most HTTP clients, including modern Node.js `fetch` implementations, automatically handle gzip compression when the server supports it.

### TypeScript (Node.js + `fetch`)

```ts
import fetch from 'node-fetch';
import { writeFileSync } from 'node:fs';

const token = 'YOUR_TOKEN';
const serverUrl = 'https://games.datsteam.dev';
const url = `${serverUrl}/api/move`;

async function move() {
  const response = await fetch(url, {
    method: 'POST',
    headers: {
      'X-Auth-Token': token,
      'Content-Type': 'application/json',
      // Optional: compression is usually negotiated automatically,
      // but you can be explicit if needed:
      'Accept-Encoding': 'gzip, deflate',
    },
    body: JSON.stringify({ snakes: [] }),
  });

  const body = await response.text();
  writeFileSync('example_response.json', body);
}

move().catch(console.error);
```

---

## Basic Strategy – Greedy Bot Example

The original specification includes a very simple example bot strategy that **does not** account for obstacles or opponents, but is useful as a starting point.

### Algorithm

1. **Get state**: send `{"snakes": []}` to retrieve the latest positions of your snakes and all visible objects.
2. **Locate head**: take the coordinate of the head of the snake you wish to control.
3. **Find nearest mandarin**: compute the **Manhattan distance** to each visible mandarin and choose the closest:

   \[
   \text{dist} = |x_{\text{target}} - x_{\text{head}}| + |y_{\text{target}} - y_{\text{head}}| + |z_{\text{target}} - z_{\text{head}}|
   \]

4. **Choose direction**: move along one axis at a time to reduce the distance.
5. **Repeat**: send the chosen direction for the next tick; once the target is reached, choose a new target.

### Example direction helper (TypeScript‑style pseudocode)

```ts
interface Point {
  x: number;
  y: number;
  z: number;
}

function getDirection(head: Point, target: Point): [number, number, number] {
  if (head.x !== target.x) {
    return [head.x < target.x ? 1 : -1, 0, 0];
  }
  if (head.y !== target.y) {
    return [0, head.y < target.y ? 1 : -1, 0];
  }
  if (head.z !== target.z) {
    return [0, 0, head.z < target.z ? 1 : -1];
  }
  return [0, 0, 0];
}
```

> **Note**: This naive strategy ignores obstacles, other snakes, and dynamic situations. In real games it will often collide and die.

---

## State Synchronization Strategies

The specification suggests two main approaches for synchronizing your local client with the server.

### Variant 1 – Always trust server state

1. Send a request with `{"snakes": []}` (or equivalent) to obtain the **full up‑to‑date state**.
2. Decide and send a move command for your snakes.
3. Wait for the next tick to complete.
4. Repeat from step 1.

- **Pros**: always uses authoritative server state.
- **Cons**: more network requests; higher sensitivity to latency.

### Variant 2 – Predict locally between updates

1. Send an initial request to get the full state of your snakes and visible objects.
2. Send a move command and **store the direction** you sent locally.
3. Wait for the tick to end.
4. **Predict** local state:
   - Take the last known coordinates.
   - Apply the stored direction (1 pixel step) to predict new positions.
   - Use this predicted state for the next decision.
5. Send the next move command and update the stored direction.
6. Periodically resync with the server to correct drift (especially after collisions or unexpected events).

- **Pros**: fewer full state fetches; smoother local simulation.
- **Cons**: local state can diverge from server state (e.g., due to collisions or missed ticks).

---

## Quick Reference

| Item                        | Value                        |
|-----------------------------|------------------------------|
| Starting snakes per player  | 3                            |
| Starting snake length       | 1 pixel                      |
| Movement speed              | 1 pixel per tick             |
| Tick duration               | ~1 second (configurable)     |
| Visibility sector size      | 30 × 30 × 30 pixels          |
| Death penalty               | −10% of current score        |
| Golden mandarin multiplier  | 10×                          |
| Suspicious mandarin range   | `rand(-5, 5)` × base price   |

---