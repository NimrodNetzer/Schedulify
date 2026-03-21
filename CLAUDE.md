# Schedulify — CLAUDE.md

## Project Overview
Schedulify is a C++17 desktop application that generates valid university course schedules based on user-selected courses and time constraints. It has an AI-powered chatbot (SchedBot) that uses the Claude API to filter schedules via natural language queries translated to SQL.

## Architecture
MVC pattern with a clean interface boundary:

- **`model_interfaces.h`** — shared data structs and `IModel` interface (the contract between model and controller)
- **`model/`** — all business logic, data access, and algorithms
  - `src/schedule_algorithm/` — core schedule generation (`CourseLegalComb`, `ScheduleBuilder`, `TimeUtils`)
  - `src/db/` — SQLite persistence via Qt SQL (`db_manager`, `db_schema`, `db_courses`, `db_schedules`, etc.)
  - `src/parsers/` — Excel (OpenXLSX), JSON (jsoncpp), CSV, and text parsers
  - `src/sched_bot/` — Claude API integration + SQL validator for natural language schedule filtering
  - `src/main/` — `model_access.cpp` implements `IModel::executeOperation()` dispatch
- **`controller/`** — Qt-side logic, connects QML view to model
  - `adapters/thread_workers/` — `ScheduleGenerator`, `CourseValidator`, `ChatBot` run heavy ops off the main thread
  - `adapters/view_models/` — Qt list models (`CourseModel`, `ScheduleModel`, `FileModel`) for QML binding
  - `adapters/filters/` — schedule filter logic
  - `src/` — one `.cpp` per screen/controller
- **`view/`** — QML UI
  - `main.qml` — root; `course_selection.qml`, `file_input.qml`, `schedules_display.qml`, `log_display.qml`
  - `popups/` — `SchedBot.qml`, `AddCoursePopup.qml`, `ExportMenu.qml`, `SlotBlockMenu.qml`, `SortMenu.qml`
- **`logger/`** — file-based logger

## Key Data Structures (model_interfaces.h)
- `Course` — holds all session groups (Lectures, Tirgulim/tutorials, labs, blocks, etc.)
- `Group` — a session type with a list of `Session` (day, start/end time, room)
- `SessionType` enum — LECTURE, TUTORIAL, LAB, BLOCK, DEPARTMENTAL_SESSION, REINFORCEMENT, GUIDANCE, OPTIONAL_COLLOQUIUM, REGISTRATION, THESIS, PROJECT
- `InformativeSchedule` — rich metrics struct stored in DB (gaps, compactness, day flags, time preferences)
- `BotQueryRequest` / `BotQueryResponse` — Claude API chatbot request/response
- `ModelOperation` enum — all operations dispatched through `IModel::executeOperation()`

## Build System
- **CMake** (3.16+), target: `SchedGUI`, C++17
- **Dependencies fetched via FetchContent**: OpenXLSX (Excel), jsoncpp
- **Find-package**: Qt6 (Core, Gui, Widgets, Quick, Qml, QuickLayouts, PrintSupport, QuickControls2, Sql), libcurl
- **Platform notes**:
  - Windows: use vcpkg for curl (`vcpkg install curl`)
  - macOS: Homebrew (`brew install curl`); universal binary (x86_64 + arm64), deployment target 11.0
  - Linux: `apt-get install libcurl4-openssl-dev` / equivalent
- UTF-8 compile flags enabled on Windows (`/utf-8` for MSVC, `-finput-charset` for MinGW)

### Build (typical)
```bash
mkdir build && cd build
cmake ..   # add -DCMAKE_TOOLCHAIN_FILE=... for vcpkg on Windows
cmake --build .
```

## Git & Branching
- Main branch: `main`
- Development branch: `devel`
- Feature branches prefixed with `f-` (e.g., `f-convert-db-to-new-structs`)

## Notes
- `model_interfaces.h` is the single source of truth for shared types — changes here affect both model and controller
- The model's `executeOperation()` uses `void*` for data in/out — cast carefully and consistently
- SchedBot translates natural language to SQL via Claude API, then validates the SQL before execution (`sql_validator.cpp`)
- Semester encoding: 1=A, 2=B, 3=Summer, 4=Yearly
- `Course::getUniqueId()` = `raw_id + "_sem" + semester` — used as the DB unique key
