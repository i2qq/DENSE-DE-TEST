# Dense Example 1: Python Script Generation & Execution

This example demonstrates how Dense generates, validates, and runs a Python script while adhering strictly to Dense's **zero-whitespace, zero-decorative-punctuation** language grammar.

## Files in this Directory

- `project.denseproj`: Mandatory Dense project manifest containing signature header, scratchpad block, and imported modules.
- `table.denselook`: Mandatory Dense lookup table specifying `style | normal`.
- `main.dense`: The Dense source script that:
  1. Creates the workspace output directory (`Q|output`).
  2. Synthesizes a valid Python script (`output\generated_script.py`) using `9` (write) and `a` (append).
  3. Reads the script back from disk into memory (`code=J|output\generated_script.py`).
  4. Branches conditionally (`has_file==&T|...`) to check for file generation.
  5. Executes the Python script in a subprocess (`status=N|python output\generated_script.py`).
  6. Writes completion status to `output\execution.log`.
- `run.bat`: Windows batch launcher.
- `run.ps1`: PowerShell launcher.

## Running Example 1

### Option 1: PowerShell
```powershell
cd "c:\Users\Victor\Antigravity Projects\Business\Prog Langs\Dense\examples\Example1"
.\run.ps1
```

### Option 2: Command Line / Batch
```cmd
cd "c:\Users\Victor\Antigravity Projects\Business\Prog Langs\Dense\examples\Example1"
run.bat
```

### Option 3: Direct interpreter execution
```powershell
..\..\dense.exe main.dense
```
