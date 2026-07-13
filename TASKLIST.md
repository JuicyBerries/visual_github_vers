Phase 1: Foundations
    [x] Initialize the Qt Widgets project using CMake.
    [x] Link the libgit2 library to the compiler.
    [x] Create the main UI skeleton with QSplitter and QStackedWidget.
    [x] Manage the Git engine lifecycle (git_libgit2_init and shutdown).

Phase 2: Settings & Security (In Progress)
    [x] Save and load local credentials using QSettings.
    [x] Create the C++ Callback function to provide the token to libgit2.
    [x] Connect the "Tester" button: ping (or git ls-remote) the remote URL to validate the GitHub token.

Phase 3: Repository State (Read-Only)
    [x] Log Page: Extract the commit history (git_revwalk).
    [x] Log Page: Format dates using QDateTime.
    [] Log Page (Upgrade): Transform the QListWidget into a QTableView with a custom C++ Model (necessary to pave the way for the visual graph).
    [] Commit Page: Read the git status via git_status_list_new() to automatically list modified/new files (like your .vrm files and textures).
    [] Branches Page: List local branches (refs/heads/ folder).
    [] Branches Page: List remote branches (refs/remotes/ folder).

Phase 4: Local Actions (Write)
    [] Drag & Drop: Create a custom C++ class inheriting from QFrame to replace your Drop Zone and catch dropEvent triggers (dragging files from Ubuntu into the app).
    [] Staging: Add files to the index (the equivalent of git add).
    [] Unstaging: Remove files from the index (if you change your mind).
    [] Commit: Wire up the "Ajouter au commit" button. Retrieve the message text, the author, and use git_commit_create() to set it in stone.

Phase 5: Network (Talking to GitHub)
    [] Fetch: Download updates from the server without modifying your local files (git_remote_fetch).
    [] Push: Send your local commits to GitHub (git_remote_push).
    [] Network UI: Add a progress bar or a loading spinner to the interface so the app doesn't freeze during network transfers (ideally using QtConcurrent).

Phase 6: The Final Boss (The Graph)
    [] Branch Algorithm: Analyze the parents of each commit to figure out which branches are merging or splitting.
    [] The Drawing: Create a QStyledItemDelegate and override its paint() method.
    [] Visual Render: Use QPainter to draw beautiful colored lines and circles (the commit nodes) directly inside the cells of your history table.
