/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   bogostrlen.c                                      :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: tomperei <tomperei@student.42lehavre.fr>  #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/09/17 22:50:01 by tomperei         #+#    #+#              */
/*   Updated: 2026/09/17 22:50:07 by tomperei        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>

static bool    is_exact_length(const char *str, int guess)
{
    int    i = 0;

    // Arrêt immédiat dès qu'on croise '\0' : évite le SIGSEGV sur les grands nombres
    while (i < guess)
    {
        if (str[i] == '\0')
            return (false);
        i++;
    }
    return (str[guess] == '\0');
}

int    bogo_strlen(const char *str)
{
    int                    guess;
    unsigned long long    attempts = 0;

    if (!str)
        return (0);
    while (1)
    {
        attempts++;
        // Tirage uniforme entre 0 et INT_MAX
        // (Sur Linux/glibc, RAND_MAX est déjà égal à INT_MAX)
        guess = rand() % INT_MAX;
        if (is_exact_length(str, guess))
        {
            printf("Succès après %llu tentatives !\n", attempts);
            return (guess);
        }
    }
}

int    main(void)
{
    const char    *test = "42";
    clock_t        start;
    clock_t        end;
    int            len;

    srand((unsigned int) time(NULL));
    printf("Calcul de la longueur de \"%s\"...\n", test);
    printf("Espace de recherche : [0, %d]\n\n", INT_MAX);
    start = clock();
    len = bogo_strlen(test);
    end = clock();
    printf("Longueur finale : %d\n", len);
    printf("Temps écoulé    : %.2f secondes\n", (double)(end - start) / CLOCKS_PER_SEC);
    return (0);
}
