/**
 * @file ./inc/server/global.h
 * @brief 
 * @author Fendy (xingfen.star@gmail.com)
 * @version 1.0
 * @date 2025-03-21
 * @copyright Copyright (c) 2025
 */
#ifndef GLOBAL_H
#define GLOBAL_H

#include <vector>

#include <HashTable.h>
#include <conn.h>
#include <list.h>

typedef struct {
    HMap db;
    /* a map of all client connections, keyed by fd */
    std::vector<Conn *> fd2conn;
    /* timers for idle connections */
    DList idle_list;
} GLOBAL_DATA;

extern GLOBAL_DATA g_data;

#endif /* GLOBAL_H */