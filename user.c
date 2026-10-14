#include "user.h"
#include "utils.h"
#include "loan.h"
#include <stdio.h>
#include <string.h>

void user_store_init(UserStore *store) { store->count = 0; }

void user_load(UserStore *store, const char *file_name) {
    FILE *file = fopen(file_name, "r"); char line[TEXT_SIZE * 3];
    if (file == NULL) return;
    while (store->count < MAX_USERS && fgets(line, sizeof(line), file) != NULL) {
        User *user = &store->items[store->count]; trim_newline(line);
        user->returned_books = 0;
        if (sscanf(line, "%d|%63[^|]|%63[^|]|%127[^|]|%d|%d", &user->id, user->username, user->password, user->name, &user->is_admin, &user->returned_books) >= 5) store->count++;
    }
    fclose(file);
}

int user_save(const UserStore *store, const char *file_name) {
    FILE *file = fopen(file_name, "w");
    if (file == NULL) { perror("无法保存用户数据"); return 0; }
    for (int i = 0; i < store->count; i++) {
        const User *user = &store->items[i];
        fprintf(file, "%d|%s|%s|%s|%d|%d\n", user->id, user->username, user->password, user->name, user->is_admin, user->returned_books);
    }
    fclose(file); return 1;
}

User *user_authenticate(UserStore *store, const char *username, const char *password) {
    for (int i = 0; i < store->count; i++) {
        User *user = &store->items[i];
        if (strcmp(user->username, username) == 0 && strcmp(user->password, password) == 0) return user;
    }
    return NULL;
}

static int user_find_index(const UserStore *store, int id) {
    for (int i = 0; i < store->count; i++) if (store->items[i].id == id) return i;
    return -1;
}

static void user_print(const User *user) {
    printf("编号: %-4d | 账号: %-16s | 姓名: %-12s | 身份: %s\n",
           user->id, user->username, user->name, user->is_admin ? "管理员" : "借阅者");
}

void user_add(UserStore *store, const char *file_name) {
    User user; int max_id = 0;
    if (store->count >= MAX_USERS) { printf("用户数量已达到上限。\n"); return; }
    for (int i = 0; i < store->count; i++) if (store->items[i].id > max_id) max_id = store->items[i].id;
    user.id = max_id + 1; user.returned_books = 0;
    read_line("请输入登录账号: ", user.username, sizeof(user.username)); read_line("请输入登录密码: ", user.password, sizeof(user.password));
    read_name("请输入借阅者姓名: ", user.name, sizeof(user.name)); user.is_admin = 0;
    store->items[store->count++] = user; user_save(store, file_name); printf("借阅者添加成功，用户编号为 %d。\n", user.id);
}

void user_update(UserStore *store, const char *file_name) {
    int id = read_int("请输入要修改的读者编号: "); int index = user_find_index(store, id);
    if (index < 0) { printf("未找到该编号的读者。\n"); return; }
    User *user = &store->items[index];
    if (user->is_admin) { printf("管理员账号不能在此修改。\n"); return; }
    printf("当前信息：\n"); user_print(user);
    read_line("新登录密码: ", user->password, sizeof(user->password));
    read_name("新借阅者姓名: ", user->name, sizeof(user->name));
    user_save(store, file_name); printf("读者信息修改成功。\n");
}

void user_delete(UserStore *store, LoanStore *loans, const char *user_file, const char *loan_file) {
    int id = read_int("请输入要删除的读者编号: "); int index = user_find_index(store, id);
    if (index < 0) { printf("未找到该编号的读者。\n"); return; }
    if (store->items[index].is_admin) { printf("不能删除管理员账号。\n"); return; }
    for (int i = 0; i < loans->count; i++) {
        if (loans->items[i].user_id == id) { printf("该读者还有未归还的图书，请先归还后再删除。\n"); return; }
    }
    printf("即将删除读者：\n"); user_print(&store->items[index]);
    for (int i = index; i < store->count - 1; i++) store->items[i] = store->items[i + 1];
    store->count--;
    for (int i = 0; i < loans->count; ) {
        if (loans->items[i].user_id == id) {
            for (int j = i; j < loans->count - 1; j++) loans->items[j] = loans->items[j + 1];
            loans->count--;
        } else i++;
    }
    user_save(store, user_file); loan_save(loans, loan_file);
    printf("读者删除成功。\n");
}

void user_list(const UserStore *store) {
    printf("\n========== 借阅者列表 ==========\n");
    for (int i = 0; i < store->count; i++) if (!store->items[i].is_admin) printf("编号: %-4d | 账号: %-16s | 姓名: %s\n", store->items[i].id, store->items[i].username, store->items[i].name);
}
