
#ifndef GPIO_H_
#define GPIO_H_

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define GPIO_DEBOUNCE_COUNT         2
#define GPIO_DIRECTION_INPUT        0
#define GPIO_DIRECTION_OUTPUT       1

uint8_t gpio_register_pin(uint8_t pin, uint8_t direction, bool pulled_up);
uint8_t gpio_read_pin(uint8_t pin);
uint8_t gpio_write_pin(uint8_t pin, bool value);

void    gpio_debouncer(void);

struct list_head
{
    struct list_head* prev;
    struct list_head* next;
};

/*
 *  Macro for getting address of a container which contains
 *  a member with type 'type'
 *  ptr: pointer to a member in a list
 *  type: type of a structure containing the member
 *  member: name of a member in a structure
 *
 *  Example:
 *
 *  struct gpio_chip
 *  {
 *      int a;
 *      int b;
 *      struct list_head list;
 *  };
 *  ...
 *  struct list_head* gpio_element =  a structure contained inside gpio_chip
 *  How to get an address of a container?
 *  ->
 *  container_of(gpio_element, struct gpio_chip, list)
 * */

#define container_of(ptr, type, member) ({                \
        const typeof( ((type*)0)->member)* __mptr = (ptr);  \
        (type*)( (char*)__mptr - offsetof(type, member));   })


#define LIST_HEAD(name)                                     \
        struct list_head name = {&(name), &(name)}

#define list_empty(name)                                    \
        name->next == name                                  \
/*
 * Macro to get the struct for this entry
 * @ptr:    the struct list_head pointer
 * @type:   type of a structure list is embedded with
 * @member: name of struct list_head inside a structure
 * */
#define list_entry(ptr, type, member)                       \
        container_of(ptr, type, member)

/*
 * Macro to get the first element from a list.
 * Keep in mind that lists are circular, and list is
 * represented by a struct list_head element.
 * For example, the way to initialize an empty list is:
 * LIST_HEAD(list_name);
 * Now list_name is, in fact, head of the list!
 * It is an entry point to a list, simply a struct
 * containing a pointer to a first and last linked-list
 * elements.
 * So to get the first entry, simply go to the next pointer
 * of head.
 */
#define list_first_entry(ptr, type, member)                 \
        list_entry((ptr)->next, type, member)

#define list_last_entry(ptr, type, member)                  \
        list_entry((ptr)->prev, type, member)

/*
 * Macro for getting the next list entry.
 * @pos:    structure holding a struct list_head
 * @member: name of a struct list_head inside struct 'pos'
 */
#define list_next_entry(pos, member)                        \
        list_entry((pos)->member.next, typeof(*(pos)), member)

/*
 * Macro for getting the previous list entry.
 * @pos:    structure holding a struct list_head
 * @member: name of a struct list_head inside struct 'pos'
 */
#define list_previous_entry(pos, member)                    \
        list_entry((pos)->member.prev, typeof(*(pos)), member)

#define list_for_each(pos, head)                            \
       for(pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_entry(pos, head, member)              \
    for(pos = list_first_entry(head, typeof(*pos), member); \
        &pos->member != (head);                               \
        pos = list_next_entry(pos, member))

static inline void __list_add(struct list_head* new,
                              struct list_head* prev,
                              struct list_head* next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

static inline void list_add(struct list_head* new, struct list_head* head)
{
    __list_add(new, head, head->next);
}

static inline void list_add_tail(struct list_head* new, struct list_head* head)
{
    __list_add(new, head->prev, head);
}

static inline void __list_del(struct list_head* prev, struct list_head* next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void __list_del_entry(struct list_head* entry)
{
    __list_del(entry->prev, entry->next);
}

static inline void list_del(struct list_head* entry)
{
    __list_del((entry)->prev, (entry)->next);
}

#endif