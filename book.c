#include "book.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

void book_store_init(BookStore *store) { store->count = 0; }

void book_load(BookStore *store, const char *file_name) {
    FILE *file = fopen(file_name, "r"); char line[TEXT_SIZE * 4];
    if (file == NULL) return;
    while (store->count < MAX_BOOKS && fgets(line, sizeof(line), file) != NULL) {
        Book *book = &store->items[store->count]; trim_newline(line);
        if (sscanf(line, "%d|%127[^|]|%127[^|]|%127[^|]|%d|%d", &book->id, book->title, book->author, book->category, &book->year, &book->available) == 6) store->count++;
    }
    fclose(file);
}

int book_save(const BookStore *store, const char *file_name) {
    FILE *file = fopen(file_name, "w");
    if (file == NULL) { perror("无法保存图书数据"); return 0; }
    for (int i = 0; i < store->count; i++) {
        const Book *book = &store->items[i];
        fprintf(file, "%d|%s|%s|%s|%d|%d\n", book->id, book->title, book->author, book->category, book->year, book->available);
    }
    fclose(file); return 1;
}

int book_find_index(const BookStore *store, int id) {
    for (int i = 0; i < store->count; i++) if (store->items[i].id == id) return i;
    return -1;
}

void book_print(const Book *book) {
    printf("编号: %-4d | 书名: %-24s | 作者: %-16s | 分类: %-10s | 年份: %d | 状态: %s\n", book->id, book->title, book->author, book->category, book->year, book->available ? "可借阅" : "已借出");
}

void book_list(const BookStore *store) {
    if (store->count == 0) { printf("当前没有图书。\n"); return; }
    printf("\n========== 图书列表 ==========\n");
    for (int i = 0; i < store->count; i++) book_print(&store->items[i]);
}

static void book_search_by_tag(const BookStore *store) {
    char tags[64][TEXT_SIZE]; int tag_count = 0; int choice; int found = 0;
    for (int i = 0; i < store->count; i++) {
        int exists = 0;
        for (int j = 0; j < tag_count; j++) if (strcmp(tags[j], store->items[i].category) == 0) { exists = 1; break; }
        if (!exists && tag_count < 64) strcpy(tags[tag_count++], store->items[i].category);
    }
    if (tag_count == 0) { printf("当前没有可用的分类标签。\n"); return; }
    printf("\n========== 分类标签 ==========\n");
    for (int i = 0; i < tag_count; i++) printf("%d. %s\n", i + 1, tags[i]);
    printf("0. 返回\n");
    choice = read_int("请选择分类标签: ");
    if (choice < 1 || choice > tag_count) return;
    printf("\n========== [%s] 分类下的图书 ==========\n", tags[choice - 1]);
    for (int i = 0; i < store->count; i++) {
        if (strcmp(store->items[i].category, tags[choice - 1]) == 0) { book_print(&store->items[i]); found = 1; }
    }
    if (!found) printf("该分类下暂无图书。\n");
}

void book_search(const BookStore *store) {
    char keyword[TEXT_SIZE]; int found = 0; int mode;
    printf("\n========== 图书检索 ==========\n");
    printf("1. 关键词模糊搜索（书名/作者/分类）\n2. 按分类标签搜索\n0. 返回\n");
    mode = read_int("请选择检索方式: ");
    if (mode == 2) { book_search_by_tag(store); return; }
    if (mode != 1) return;
    read_line("请输入书名、作者或分类关键词: ", keyword, sizeof(keyword));
    printf("\n========== 检索结果 ==========\n");
    for (int i = 0; i < store->count; i++) {
        const Book *book = &store->items[i];
        if (contains_ignore_case(book->title, keyword) || contains_ignore_case(book->author, keyword) || contains_ignore_case(book->category, keyword)) { book_print(book); found = 1; }
    }
    if (!found) printf("没有找到匹配的图书。\n");
}

void book_add(BookStore *store, const char *file_name) {
    Book book; int max_id = 0;
    if (store->count >= MAX_BOOKS) { printf("图书数量已达到上限。\n"); return; }
    for (int i = 0; i < store->count; i++) if (store->items[i].id > max_id) max_id = store->items[i].id;
    book.id = max_id + 1;
    read_line("请输入书名: ", book.title, sizeof(book.title));
    read_line("请输入作者: ", book.author, sizeof(book.author));
    read_line("请输入分类: ", book.category, sizeof(book.category));
    book.year = read_int("请输入出版年份: "); book.available = 1;
    store->items[store->count++] = book; book_save(store, file_name);
    printf("图书添加成功，编号为 %d。\n", book.id);
}

void book_update(BookStore *store, const char *file_name) {
    int id = read_int("请输入要修改的图书编号: "); int index = book_find_index(store, id);
    if (index < 0) { printf("未找到该编号的图书。\n"); return; }
    Book *book = &store->items[index]; printf("当前信息：\n"); book_print(book);
    read_line("新书名: ", book->title, sizeof(book->title)); read_line("新作者: ", book->author, sizeof(book->author));
    read_line("新分类: ", book->category, sizeof(book->category)); book->year = read_int("新出版年份: ");
    book_save(store, file_name); printf("图书修改成功。\n");
}

void book_delete(BookStore *store, const char *file_name) {
    int id = read_int("请输入要删除的图书编号: "); int index = book_find_index(store, id);
    if (index < 0) { printf("未找到该编号的图书。\n"); return; }
    if (!store->items[index].available) { printf("该图书已借出，请归还后再删除。\n"); return; }
    for (int i = index; i < store->count - 1; i++) store->items[i] = store->items[i + 1];
    store->count--; book_save(store, file_name); printf("图书删除成功。\n");
}
