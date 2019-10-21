#include <stdio.h>
#include <string.h>
#include <ctype.h>

struct Node
{
	char *data;
	struct Node *ltree;
	struct Node *rtree;
};

struct Node *head;
struct Node *newTree(char *p)
{
	struct Node *n = malloc(sizeof(struct Node));
	n->data = malloc(strlen(p) + 1);
	strcpy(n->data, p);
	n->ltree = n->rtree = NULL;
	return n;
}

struct Node *insert(struct Node *tree, char *p)
{
	if (tree == NULL)
		return newTree(p);
	int cmp = strcmp(p, tree->data);
	if (cmp <= 0)
		tree->ltree = insert(tree->ltree, p);
	else
		tree->rtree = insert(tree->rtree, p);
	return tree;
}

void inorder(struct Node *tree)
{
	if (tree == NULL)
		return;
	inorder(tree->ltree);
	printf("%s\n", tree->data);
	inorder(tree->rtree);
}

void main()
{
	char letter;
	char word[100];
	int counter = 0;
	char prior = ' ';
	char lowerCase=' '; 
	FILE *fptr;
	if ((fptr = fopen("alphabetise_names.txt", "r")) == NULL)
	{
		printf("Error. Empty File");
		return;
	}

	while (letter != EOF)
	{

		letter = fgetc(fptr);
		if (letter == ' ' || letter == '\n' || letter == EOF)
		{
			if (prior != ' ' && prior != '\n' && prior != EOF)
			{
				word[counter] = '\0';
				counter = 0;
				head = insert(head, word);
			}
		}
		else
		{
			lowerCase = tolower(letter);
			word[counter] = lowerCase;
			counter++;
		}
		prior = letter;
	}

	inorder(head);
	return;
}
