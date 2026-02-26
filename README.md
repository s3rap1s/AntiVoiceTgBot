# AntiVoiceTgBot

Telegram inline bot that “plays” text like a voice note: it reveals your message gradually (chunk-by-chunk) at a selected speed, with an inline keyboard to **show the full text** or **replay**.
This bot was created with a single purpose - to counter people who abuse voice messages.
Built with **C++23**, **CMake**, and **tgbot-cpp**.

## What it does

- **Inline mode**: type `@anti_VM_bot some text` in any chat → choose a speed preset.
- **Gradual reveal**: the bot edits the inline message repeatedly, revealing more words over time.
- **Long text flow**: save a longer text in a private chat, then send it via inline mode (Telegram inline mode limitations).
- **Accumulative mode**: keep the already-revealed text in the message while it grows.
- **Inline keyboard actions**:
  - **Show full text** (opens the full message in an alert)
  - **Listen again** (replays gradual reveal)


## Bot commands

In private chat with the bot:

- `/start` — help + speed presets
- `/save` — start long-text flow (send the text after this command)
- `/clear` — delete saved text
- `/a` — **option** for inline query to enable “accumulative” mode (`@anti_VM_bot /a your text`)

## How to use

### 1) Inline mode (quick messages)
1. In any chat, type: `@anti_VM_bot hello this is a test`
2. Pick a speed preset from the inline results
3. The message will start “playing” (gradually updating)

### 2) Long text flow
1. Send `/save` to the bot
2. Send the long text
3. In any chat, type: `@anti_VM_bot` (empty query)
4. Pick a speed preset

### 3) Accumulative mode
Use `/a` at the beginning of the inline query:
- `@anti_VM_bot /a this text will accumulate`

## TODO
* Storage is in-memory (saved texts and message state reset on restart)
* “Premium” is currently a prototype

# Future plans
For a closer look at asynchrony, I decided to create this bot using the userver framework, where modern technologies will already be added. [Repo with userver framework](https://github.com/s3rap1s/AntiVoiceTgBotUserver)
