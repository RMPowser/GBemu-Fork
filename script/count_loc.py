import os
import sys

def count_lines_of_code(root_dir, extensions):
    """
    Recursively counts the total lines of code in files matching the given extensions.

    Args:
        root_dir (str): The starting directory for the search.
        extensions (tuple): A tuple of file extensions (e.g., ('.cpp', '.h')).

    Returns:
        tuple: (total_lines_count, total_files_count, list_of_file_results)
    """
    total_lines = 0
    file_count = 0
    file_results = [] # Stores (line_count, filepath) for later sorting and display

    print(f"--------------------------------------------------------")
    print(f"Starting Recursive Line of Code (LOC) Count")
    print(f"Target extensions: {', '.join(extensions)}")
    print(f"Searching from: {os.path.abspath(root_dir)}")
    print(f"--------------------------------------------------------")

    # os.walk efficiently traverses the directory tree recursively
    for dirpath, dirnames, filenames in os.walk(root_dir):

        # Optionally skip common development directories to speed up the scan
        # You can add more directories here if needed.
        dirnames[:] = [d for d in dirnames if d not in ('.git', 'build', 'node_modules', 'venv', 'blargg_roms')]

        for filename in filenames:
            # Check if the filename ends with any of the target extensions (case-insensitive check)
            if filename.lower().endswith(extensions):
                filepath = os.path.join(dirpath, filename)

                try:
                    # Open the file and count lines efficiently
                    # 'errors=ignore' is used to handle files that might contain binary data
                    with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                        # The sum(1 for line in f) pattern is the fastest way to count lines in Python
                        line_count = sum(1 for line in f)
                        total_lines += line_count
                        file_count += 1
                        # Store the individual file result
                        file_results.append((line_count, filepath))

                except Exception as e:
                    # Catch permissions errors, unusual file system errors, etc.
                    print(f"Skipping file due to error: {filepath} ({e})", file=sys.stderr)

    # Return the file results along with the totals
    return total_lines, file_count, file_results

# --- Main Execution ---
if __name__ == "__main__":
    # Define target extensions as a tuple for efficient checking
    TARGET_EXTENSIONS = ('.cpp', '.h', '.inc')

    # Start search from the current working directory ('.')
    start_directory = "."

    # Unpack the three return values from the function
    total, files, results = count_lines_of_code(start_directory, TARGET_EXTENSIONS)

    print(f"--------------------------------------------------------")

    if files > 0:
        print("\n--- Files Sorted by Line Count (Descending) ---")

        # Sort the results: sort by the first element of the tuple (line count) in reverse order
        sorted_results = sorted(results, key=lambda x: x[0], reverse=True)

        # Print the detailed list
        for line_count, filepath in sorted_results:
            # Using ljust to nicely align the line count column
            print(f"{str(line_count).ljust(8)} {filepath}")

        print("\n--- Summary ---")

    if files == 0:
        print("Result: 0")
        print("No matching files found or all matching files are empty.")
    else:
        print(f"Total files analyzed: {files}")
        print(f"Result: {total}")
        print(f"Total lines counted across all matching files.")
    print(f"--------------------------------------------------------")
