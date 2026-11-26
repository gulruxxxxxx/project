# Presenting QueueLess

## 60-second pitch
"Students lose time standing in queues without knowing how long they will be.
QueueLess tells you the wait before you leave your seat - and tells staff
where to open another counter. The brain is C++; the browser is just the screen."

## Demo script (about 3 minutes)
1. Open the dashboard next to the terminal. Point at the sentence at the top: it picks the best service.
2. Terminal -> **2 Join a queue** (Cafeteria). Show the ticket. The dots on the web page grow by one.
3. **3 Track my place** with the same ID: position, people ahead, "be back at" time.
4. **8 Staff panel** (PIN 1234) -> serve next / open a counter. Wait times drop on the page.
5. Click a service on the web page, drag the counter slider: "same formula as `calcWait()`".
6. **Invalid input (required by the brief):** at a menu type `abc`, then `99`, then a wrong staff PIN. Point out that the program stays calm and asks again.
7. **9 -> 2 Live demo**: the whole page moves for 30 seconds. Let it run while you explain the code.

## Who explains what (split it so both of you speak)
- Person A: problem, formula, `queue.cpp`, `storage.cpp` (files), demo steps 1-4.
- Person B: `helpers.cpp` (validation), `analytics.cpp`, `dashboard.cpp` (C++ -> data.js -> browser), demo steps 5-6.
Both: be able to read `calcWait()` and `readInt()` line by line.

## Questions to expect
- **Why C++ and not just a website?** All logic, validation and saving is C++; the page only displays data.
- **How does the page know about changes?** After each change C++ rewrites `web/data.js`; the page reloads it every 2 seconds.
- **What if the file is damaged?** `loadQueues()` checks every value and falls back to sample data.
- **What if someone types letters?** `readInt()` reads a whole line and rejects anything that is not exactly one number in range.
- **Is the estimate accurate?** It assumes average service time per person; the more history, the better the "best time" advice. Real timestamps would be the next improvement.
- **What happens when history is full?** The oldest record is dropped, the newest are kept.
- **Is the PIN secure?** It is a demo PIN in the source code; a real system would store a hash.
- **What would you add next?** SMS/push notification when you are 3rd in line; real service timing instead of averages.
