#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "fast_timer.h"

int fast_timer_init(FastTimer *timer, const int slot_count,
    const int64_t current_time)
{
  int bytes;
  if (slot_count <= 0 || current_time <= 0) {
    return EINVAL;
  }

  timer->slot_count = slot_count;
  timer->base_time = current_time; //base time for slot 0
  timer->current_time = current_time;
  bytes = sizeof(FastTimerSlot) * slot_count;
  timer->slots = (FastTimerSlot *)malloc(bytes);
  if (timer->slots == NULL) {
     return errno != 0 ? errno : ENOMEM;
  }
  memset(timer->slots, 0, bytes);
  return 0;
}

void fast_timer_destroy(FastTimer *timer)
{
  if (timer->slots != NULL) {
    free(timer->slots);
    timer->slots = NULL;
  }
}

int fast_timer_add(FastTimer *timer, const int64_t expires,
    FastTimerEntry *entry)
{
  FastTimerSlot *slot;
  slot = timer->slots + (expires - timer->base_time) % timer->slot_count;
  entry->next = slot->head.next;
  if (slot->head.next != NULL) {
    slot->head.next->prev = entry;
  }
  entry->prev = &slot->head;
  slot->head.next = entry;
  return 0;
}

int fast_timer_remove(FastTimer *timer, FastTimerEntry *entry)
{
  if (entry->prev == NULL) {
     return ENOENT;   //already removed
  }

  if (entry->next != NULL) {
     entry->next->prev = entry->prev;
     entry->prev->next = entry->next;
     entry->next = NULL;
  }
  else {
     entry->prev->next = NULL;
  }

  entry->prev = NULL;
  return 0;
}

FastTimerSlot *fast_timer_slot_get(FastTimer *timer, const int64_t current_time)
{
  if (timer->current_time >= current_time) {
    return NULL;
  }

  return timer->slots + (timer->current_time++ - timer->base_time) %
     timer->slot_count;
}

int fast_timer_timeouts_get(FastTimer *timer, const int64_t current_time,
   FastTimerEntry *head)
{
  FastTimerSlot *slot;
  FastTimerEntry *entry;
  FastTimerEntry *first;
  FastTimerEntry *last;
  FastTimerEntry *tail;
  int count;

  head->prev = NULL;
  head->next = NULL;
  if (timer->current_time >= current_time) {
    return 0;
  }

  first = NULL;
  last = NULL;
  tail = head;
  count = 0;
  while (timer->current_time < current_time) {
    slot = timer->slots + (timer->current_time++ - timer->base_time) %
       timer->slot_count;
    entry = slot->head.next;
    while (entry != NULL) {
      if (entry->expires >= current_time) {
         if (first != NULL) {
            first->prev->next = entry;
            entry->prev = first->prev;

            tail->next = first;
            first->prev = tail;
            tail = last;
            first = NULL;
         }
      }
      else {
         count++;
         if (first == NULL) {
            first = entry;
         }
      }
      last = entry;
      entry = entry->next;
    }

    if (first != NULL) {
       first->prev->next = NULL;

       tail->next = first;
       first->prev = tail;
       tail = last;
       first = NULL;
    }
  }

  return count;
}

