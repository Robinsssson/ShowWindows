//
// Created by Robinson on 2023/11/22.
//

#include "SingletonMatQueue.h"

QMutex SingletonMatQueue::m_mutex;
SingletonMatQueue *SingletonMatQueue::m_instance;
