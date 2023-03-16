/*Задание 3. Симуляция работы кухни онлайн-ресторана

Что нужно сделать
Требуется реализовать упрощённую модель работы кухни ресторана, работающего
в режиме онлайн-доставки.
Онлайн-заказ поступает каждые 5–10 секунд. Это случайное блюдо из пяти: пицца,
суп, стейк, салат, суши. Официант, принявший заказ онлайн, оставляет его у кухни.
Если кухня свободна, она принимает заказ и спустя 5–15 секунд возвращает уже
готовое блюдо на выдачу. Время определяется случайным образом с помощью функции
std::rand() в указанных диапазонах.
Курьер приезжает каждые 30 секунд, забирает готовые блюда на выдаче и развозит
их по заказчикам.
Программа завершается после 10 успешных доставок. На каждом из этапов, будь то
заказ, или готовка, или доставка заказа, выводятся соответствующие сообщения в
консоль.*/

#include <iostream>
#include <string>
#include <deque>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>

std::deque<std::string> orderQueue;
std::deque <std::string> courierQueue;
std::mutex orderQueueMutex;
std::mutex courierQueueMutex;
std::mutex coutMutex;

void QueueOrdersKitchen() {

    std::deque<std::string> menu{ "pizza", "soup", "steak", "salad", "sushi" };

    std::string dish = menu.at((rand() % menu.size()));

    int orderTime = 5 + (rand() % 6);

    std::this_thread::sleep_for(std::chrono::seconds(orderTime));

    coutMutex.lock();
    std::cout << "Add to order queue dish - " << dish << std::endl;
    coutMutex.unlock();

    orderQueueMutex.lock();
    orderQueue.push_back(dish);
    orderQueueMutex.unlock();
}

void Orders() {

    for (int i = 0; i < 10; i++) {

        QueueOrdersKitchen();
    }
}

void Kitchen(std::thread* ordersId) {

    while (!orderQueue.empty() || ordersId->joinable()) {

        int kitchenTime = 5 + (rand() % 11);

        std::this_thread::sleep_for(std::chrono::seconds(kitchenTime));

        if (!orderQueue.empty()) {

            coutMutex.lock();
            std::cout << "  A dish has been removed from the order queue - " << orderQueue.at(0) << std::endl;
            coutMutex.unlock();

            courierQueueMutex.lock();
            courierQueue.push_back(orderQueue.at(0)); //Добавляем в конец для курьера блюдо 
            courierQueueMutex.unlock();

            orderQueueMutex.lock();
            orderQueue.pop_front(); //Забираем с начала очереди на кухню блюдо
            orderQueueMutex.unlock();
        }
    }
}

void Courier(std::thread* kitchen) {

    const int COURIER_TIME{ 30 };

    while (!courierQueue.empty() || kitchen->joinable()) {

        std::this_thread::sleep_for(std::chrono::seconds(COURIER_TIME));

        if (!courierQueue.empty()) {

            coutMutex.lock();
            std::cout << "      The courier delivered the dish - " << courierQueue.at(0) << std::endl;
            coutMutex.unlock();

            courierQueueMutex.lock();
            courierQueue.pop_front();
            courierQueueMutex.unlock();
        }
    }
}

int main(std::thread* ordersId)
{
    srand(time(NULL));

    std::thread orders(Orders);

    std::thread kitchen(Kitchen, &orders);

    std::thread courier(Courier, &kitchen);

    orders.join();

    kitchen.join();

    courier.join();
}

