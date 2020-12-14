/*
 * Implementation of the word_count interface using Pintos lists and pthreads.
 *
 * You may modify this file, and are expected to modify it.
 */

/*
 * Copyright © 2019 University of California, Berkeley
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PINTOS_LIST
#error "PINTOS_LIST must be #define'd when compiling word_count_lp.c"
#endif

#ifndef PTHREADS
#error "PTHREADS must be #define'd when compiling word_count_lp.c"
#endif

#include "word_count.h"

void init_words(word_count_list_t *wclist) {
    /* TODO */
    list_init(&wclist->lst);
    pthread_mutex_init(&wclist->lock, NULL);
}

size_t len_words(word_count_list_t *wclist) {
    /* TODO */
    return list_size(&wclist->lst);
}

word_count_t *find_word(word_count_list_t *wclist, char *word) {
    /* TODO */
    struct list_elem *ptr = list_begin(&wclist->lst);
    while (ptr != list_end(&wclist->lst)) {
        word_count_t *temp = list_entry (ptr, word_count_t, elem);
        if (strcmp(temp->word, word) == 0) return temp;
        ptr = list_next(ptr);
    }
    return NULL;
}

word_count_t *add_word(word_count_list_t *wclist, char *word) {
    /* TODO */
    word_count_t *find_res = find_word(wclist, word);
    pthread_mutex_lock(&wclist->lock);
    if (find_res != NULL) {
        find_res->count = find_res->count + 1;
    } else {
        find_res = malloc(sizeof(word_count_t));
        find_res->count = 1;
        find_res->word = word;
        list_push_back(&wclist->lst, &find_res->elem);
    }
    pthread_mutex_unlock(&wclist->lock);
    return find_res;
}

void fprint_words(word_count_list_t *wclist, FILE *outfile) {
    /* TODO */
    struct list_elem *ptr = list_begin(&wclist->lst);
    while (ptr != list_end(&wclist->lst)) {
        word_count_t *temp = list_entry (ptr, word_count_t, elem);
        fprintf(outfile, "word: %s\tcount: %d\n", temp->word, temp->count);
        ptr = list_next(ptr);
    }
}

static bool less_list(const struct list_elem *ewc1,
                      const struct list_elem *ewc2, void *aux) {
    /* TODO */
    word_count_t *wc1 = list_entry(ewc1, word_count_t, elem);
    word_count_t *wc2 = list_entry(ewc2, word_count_t, elem);
    if (wc1->count != wc2->count) {
        return wc1->count < wc2->count;
    } else {
        return strcmp(wc1->word, wc2->word) < 0;
    }
}

void wordcount_sort(word_count_list_t *wclist,
                    bool less(const word_count_t *, const word_count_t *)) {
    /* TODO */
    list_sort(&wclist->lst, less_list, less);
}
