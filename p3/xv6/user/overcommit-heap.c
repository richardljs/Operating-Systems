#include "types.h"
#include "user.h"

typedef struct node {
    int x;
    struct node *next;
} Node;

int 
main(int argc, char *argv[])
{
	int i = 0;
	Node *new_node = (Node *)malloc(sizeof(Node));
	Node *head = new_node;
	head->x = 0;
	head->next = NULL;
	printf(1, "address of first node : %d\n", head);

	while(i < 100000) {
		Node *next_node = (Node *)malloc(sizeof(Node));
        	next_node->x = i + 1;
        	next_node->next = head;
        	head = next_node;
		i ++;
	}
	printf(1, "address of last node : %d\n", head);
	exit();
}
