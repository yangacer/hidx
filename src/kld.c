#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/param.h>
#include <sys/systm.h>

static int load(struct module* module, int _c, void* arg) {
  int ec = 0;
  switch (_c) {
    case MOD_LOAD:
      break;
    case MOD_UNLOAD:
      break;
    default:
      ec = EOPNOTSUPP;
      break;
  }
  return ec;
}

static moduledata_t module = {"hidx", load, NULL};

DECLARE_MODULE(hidx, module, SI_SUB_DRIVERS, SI_ORDER_MIDDLE);
MODULE_VERSION(hidx, 1);
