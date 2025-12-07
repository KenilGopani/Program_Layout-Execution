/**
 * Recursive Factorial Program
 * 
 * This C program demonstrates recursion with a factorial calculation.
 * The corresponding assembly implementation is in factorial.asm
 * 
 * Assignment: Create a simple C function that has recursion - factorial calculation
 *             Create the driver (main program) that calls this function
 */

#include <stdio.h>

/**
 * Recursive factorial function
 * 
 * @param n - The number to calculate factorial of
 * @return n! (factorial of n)
 * 
 * Algorithm:
 * - Base case: if n <= 1, return 1
 * - Recursive case: return n * factorial(n-1)
 */
int factorial(int n) {
    // Base case: factorial(1) = 1, factorial(0) = 1
    if (n <= 1) {
        return 1;
    }
    
    // Recursive case: n! = n * (n-1)!
    return n * factorial(n - 1);
}

/**
 * Main driver program
 * 
 * Demonstrates calling the recursive factorial function
 * and displaying the result
 */
int main() {
    int n = 5;
    int result;
    
    // Call the recursive factorial function
    result = factorial(n);
    
    // Display the result
    printf("F(%d) = %d\n", n, result);
    
    return 0;
}
