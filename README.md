# **PrimeGenIS: Prime Generation by Incremental Search**

## **Overview**
This repository contains the code and implementations for the paper **"Prime Generation by Incremental Search: An Experimental Exploration"**. The project provides a comprehensive analysis of **Prime Generation Algorithms (PGAs)**, particularly focusing on their efficiency and suitability for **public-key cryptosystems**.

With the increasing computational power and the widespread use of cryptographic protocols relying on large prime numbers, optimizing **prime and safe prime generation** is critical. Our work evaluates **nine different PGAs**, including those implemented in the **OpenSSL** and **NSS** cryptographic libraries, to determine their practical viability.

## **Key Findings**
- **Public-key cryptography relies on large primes** due to their number-theoretic properties.
- We implemented and analyzed **nine PGAs**, assessing their strengths and weaknesses.
- While several PGAs, including **OpenSSL** and **NSS**, perform well for general prime generation, fewer implementations are suitable for **safe prime generation**, which is crucial for **Diffie-Hellman key exchange and other security protocols**.

## **Repository Structure**
The repository is organized as follows:

📂 **`src/`** – Source code implementations of different PGAs  
📂 **`benchmarks/`** – Performance analysis and benchmarking results  
📂 **`docs/`** – Research papers and documentation related to PGAs  
📂 **`tests/`** – Test cases for verifying prime generation correctness  
📂 **`plots/`** – Graphical analysis of execution time and performance trends  

## **Getting Started**
### **Installation & Setup**
To run the implementations locally, follow these steps:

```sh
git clone https://github.com/pr0f3ss/PrimeGenIS.git
cd PrimeGenIS
```

## **Prime Generation Algorithms Implemented**
This project implements and evaluates the following PGAs:
- **NSS**
- **Natural**
- **OpenSSL**

in combination with the following sieves:

- **NSS**
- **OpenSSL**
- **Dirichlet**

## **Safe Prime Generation**
Safe primes (primes `p` where `(p-1)/2` is also prime) are essential for cryptographic protocols such as **Diffie-Hellman key exchange**. We evaluate how different PGAs perform in generating safe primes and analyze their efficiency.

## **Contributing**
We welcome contributions! Feel free to:
- Open an **issue** for bug reports or feature suggestions.
- Submit **pull requests** for code improvements or additional PGAs.

## **License**
📜 This project is open-source and distributed under the **MIT License**.

---
**Authors**: Filip Dobrosavljevic  
**Paper**: "Prime Generation by Incremental Search: An Experimental Exploration"

