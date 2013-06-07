#include <QTextCodec>
#include "gaia.h"
#include "algorithms/analyzer.h"
#include "algorithms/applier.h"
#include "metrics/distancefunction.h"
#include "metrics/frozendistance.h"
#include "searchspacepool.h"


#ifdef HAVE_TBB
#include <tbb/task_scheduler_init.h>

static tbb::task_scheduler_init* tbbinit = 0;
#endif // HAVE_TBB

namespace gaia2 {

bool verbose;

const char* version = GAIA_VERSION;

// message handler that aborts execution, so that when run inside gdb we can
// still get a stack trace. Very useful!
// got the idea from http://der-dakon.net/blog/KDE/tracing-qassert.html
static void qtMessageHandler(QtMsgType type, const char *msg) {
  fprintf(stderr, "%s\n", msg);
  if (type == QtFatalMsg) {
    abort();
  }
}


void init() {
  static bool initialized = false;
  if (initialized) return;

  QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));

  qInstallMsgHandler(qtMessageHandler);

  registerAnalyzers();
  registerAppliers();
  registerMetrics();
  registerFrozenMetrics();

  // by default, verbose = true (people know what's going on)
  verbose = true;

  // set debug level
  activatedDebugLevels = GNone
    //| GMemory
    //| GAlgorithms
    //| GMath
    //| GParser

    //| GDataSet
    //| GDescriptorTree
    //| GRegion
    //| GPoint
    //| GView

    //| GAll
    ;

#ifdef HAVE_TBB
  delete tbbinit;
  tbbinit = new tbb::task_scheduler_init;
#endif

  initialized = true;
}

void shutdown() {
  SearchSpacePool::clear();
  FrozenSearchSpacePool::clear();

#ifdef HAVE_TBB
  delete tbbinit;
  tbbinit = 0;
#endif
}

} // namespace gaia2
