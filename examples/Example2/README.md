# Dense Example 2: Native Tkinter Unit Converter GUI

This example showcases Dense executing an interactive GUI application **natively with ZERO Python (`.py`) files** created or executed on disk.

Dense integrates the C API engine directly, enabling high-performance token-quantized instructions to create, manipulate, style, and loop GUI windows directly in Dense's zero-whitespace, zero-punctuation language.

## Token Quota Savings
Instead of generating hundreds of verbose Python syntax tokens (decorators, parentheses, commas, colons, indents, quotes):
```python
import tkinter as tk
root = tk.Tk()
root.title("Unit Converter")
root.geometry("380x300")
root.configure(bg="#1e1e1e")
lbl = tk.Label(root, text="DENSE_UNIT_CONVERTER", fg="#00e676", bg="#1e1e1e")
lbl.pack(pady=12)
```
Dense compresses this entire pipeline into token-quantized zero-punctuation statements:
```dense
win=TK|Tk
win|title|DenseUnitConverter
win|geometry|380x300
win|configure|bg=#1e1e1e
hdr=TK|Label|win|text=DENSE_UNIT_CONVERTER|fg=#00e676|bg=#1e1e1e
hdr|pack|pady=12
```

## Files in this Directory

- `project.denseproj`: Declares `tkinter - TK` module alias.
- `table.denselook`: Standard symbol lookup table.
- `main.dense`: Pure native Dense script building and running the UI.
- `run.bat`: Windows batch runner.
- `run.ps1`: PowerShell runner.

## Running Example 2

From PowerShell:
```powershell
cd "c:\Users\Victor\Antigravity Projects\Business\Prog Langs\Dense\examples\Example2"
powershell -ExecutionPolicy Bypass -File .\run.ps1
```

Or run `run.bat` / launch directly:
```cmd
..\..\dense.exe main.dense
```
