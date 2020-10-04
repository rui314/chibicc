#include <stdio.h>

void main () {

char grade;

printf("Enter your grade : ");
scanf(" %s", &grade);

switch(toupper(grade)) {

case 'A':
    printf("Wow you so smart!\n");
    break;

case 'B':
    printf("Enought smart!\n");
    break;

case 'C':
    printf("Are youc cheating?\n");
    break;

case 'D':
    printf("Keep Learn!\n");
    break;

default:
    printf("You enter wrong grade!\n");
}

printf("Copyright 2020 ramadhan-xic\n");




}
