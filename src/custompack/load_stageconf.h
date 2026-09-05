#pragma once

namespace custompack::stageconf {

// Load stageconf. Called after stagedef loads but before GMA/TPL are loaded so we have game heap
// space for parsing
void stage_init();

}  // namespace custompack::stageconf
