# Mini Search Engine

## What it does

Mini Search Engine is a C++17 application that searches local text documents and ranks matching files by relevance. It supports case-insensitive words and Boolean queries using `AND`, `OR`, and `NOT`.

The application indexes `.txt` files directly inside a selected folder. It handles basic English letters and digits. Phrase searches, parentheses, subfolder indexing, and web searches are outside its scope.

## How it works

The program reads each document and splits its contents into lowercase words. Punctuation separates words. It builds an **inverted index** that maps each word to the documents containing it and its count in each document.

Queries use these document lists to select matches. `AND` keeps documents present on both sides, `OR` combines the lists, and `NOT` excludes documents containing a word. Operators are processed in the order `NOT`, `AND`, then `OR`.

Matching documents receive a **TF-IDF score**. Term frequency measures a word's count relative to the document's length. Inverse document frequency gives more weight to words found in fewer documents. The score adds these contributions for each distinct positive query word. Negated words only filter results.

Results are sorted by score, with filenames used to break ties. The index is held in memory and rebuilt when the application starts.
