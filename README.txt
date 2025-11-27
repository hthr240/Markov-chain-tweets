# Markov Chain Text Generator | C System Programming

A high-performance, generic **Markov Chain** engine written in **C**. Designed to analyze large text datasets and generate realistic sentence structures using probabilistic modeling. The core engine is **generic**, capable of handling different types of state transitions (e.g., Text Generation or Board Game simulations like Snakes & Ladders).

## 🚀 Key Features
* **Generic Architecture:** The `MarkovChain` struct uses `void*` pointers to manage any data type, demonstrating **polymorphism in C**.
* **Memory Management:** Manual allocation/deallocation (`malloc`/`free`) of all nodes and strings, verified leak-free using **Valgrind**.
* **Custom Data Structures:** Implemented a **Linked List** library from scratch to handle dynamic state transitions efficiently.
* **Scalable Parsing:** capable of processing large text corpuses (e.g., `justdoit_tweets.txt`) to build statistical models.

## 🛠️ Tech Stack
* **Language:** C (C99 Standard)
* **Build System:** Makefile
* **Debugging:** GDB, Valgrind
* **Concepts:** Pointers, Dynamic Memory, Probability, Linked Lists

## 📂 Project Structure
* `tweets_generator.c`: CLI entry point for text generation. Reads a corpus and outputs random sentences.
* `snakes_and_ladders.c`: A second implementation showing the engine's ability to simulate board game paths.
* `markov_chain.c`: The core logic engine (Next State logic, Frequency Analysis).
* `linked_list.c`: A generic linked list implementation for storing state history.
