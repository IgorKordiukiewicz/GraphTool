#include "../3rdParty/doctest.h"
#include "../Include/Event.hpp"

TEST_CASE("Event") {
	class Foo
	{
	public:
		void params_0() { incr_x();  }
		void params_2(int a, int b) { set_x(a, b); }
		void incr_x() { ++x; }
		void set_x(int a, int b) { x = a + b; }

		int x{ 0 };
	} foo;

	SUBCASE("No params emit") {
		Event<> event;
		CHECK(foo.x == 0);
		event.connect("delegate", &foo, &Foo::params_0);
		event.emit();
		CHECK(foo.x == 1);
	}

	SUBCASE("2 Params emit") {
		Event<int, int> event;
		CHECK(foo.x == 0);
		event.connect("delegate", &foo, &Foo::params_2);
		event.emit(2, 5);
		CHECK(foo.x == 7);
	}

	SUBCASE("Multiple delegates") {
		Event<> event;
		event.connect("delegate", &foo, &Foo::params_0);
		event.connect("delegate2", &foo, &Foo::params_0);
		event.connect("delegate2", &foo, &Foo::params_0);
		event.emit();
		CHECK(foo.x == 2);
		CHECK(event.getDelegatesCount() == 2);

		SUBCASE("Disconnect") {
			event.disconnect("delegate2");
			event.emit();
			CHECK(foo.x == 3);
			CHECK(event.getDelegatesCount() == 1);
			event.disconnectAll();
			event.emit();
			CHECK(foo.x == 3);
			CHECK(event.getDelegatesCount() == 0);
		}
	}
}