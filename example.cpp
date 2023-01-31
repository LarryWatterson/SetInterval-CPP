#include <intervalTimer.hpp>

auto BroadcastNotice(const std::string& str) {
	std::cout << str << std::endl;
}

auto createTimer() -> void {
	const TEventInfo info{
		.callback = [&]() -> void { BroadcastNotice("Test Timer 1"); },
		.interval = INTERVAL_SEC(10),
		.lastTime = std::chrono::system_clock::now(),
		.isAlwaysUpdate = true
	};
	SetInterval("timer1", info);
}

auto createTimerWithLambda() -> void {
	setInterval("timer2", [&]() -> void { BroadcastNotice("Test Timer 2"); }, INTERVAL_SEC(5), false);
}

int main() {

	createTimer();
	createTimerWithLambda();
	return 0;
}