Import("env")


def simulate(*args, **kwargs):
    print("simulating simulation")


env.AddTarget(
        name="sim",
        dependencies=None,
        actions=[simulate],
        title="Simulate",
        description="Execute simulation with simavr",
)
