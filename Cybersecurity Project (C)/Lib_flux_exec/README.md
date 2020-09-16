# Lib_flux_exec

Set of functions for synchronizing detached threads.

- mutual exclusion semaphores gestion : initialization, P, V actions
- signal management: taking pthread_cond_wait into account
- detached threads with an intermediate function to protect the argument (guarantee its lifespan)
