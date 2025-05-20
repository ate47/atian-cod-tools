#include <includes_shared.hpp>
#include <gsc/gsc_gdb.hpp>

namespace shared::gsc::acts_debug {
	void GetGDBInfo(uint32_t rloc, GSC_ACTS_DEBUG* dbg, const char** pfilename, uint32_t* pline) {
		if (!dbg || dbg->GetMagic() != MAGIC || !dbg->HasFeature(ADF_LINES)) return; // no debug data
		
		const GSC_ACTS_LINES* lines{ dbg->GetLines() };
		const GSC_ACTS_LINES* linesEnd{ dbg->GetLinesEnd() };

		for (; lines != linesEnd; lines++) {
			if (rloc < lines->start && rloc >= lines->end) {
				continue;
			}

			if (pline) *pline = (uint32_t)lines->lineNum;
				
			if (!dbg->HasFeature(ADF_FILES) || !pfilename) {
				break; // no files, we can leave it there
			}
		
			// search the file (for multi-file scripts)
			const GSC_ACTS_FILES* files{ dbg->GetFiles() };
			const GSC_ACTS_FILES* filesEnd{ dbg->GetFilesEnd() };

			for (; files != filesEnd; files++) {
				if (lines->lineNum < files->lineStart && lines->lineNum >= files->lineEnd) {
					continue;
				}
				// we need to relativise the line
				if (pline) *pline = (uint32_t)(lines->lineNum - files->lineStart);
				*pfilename = (const char*)(dbg->magic + files->filename);
				break;
			}

			break;
		}
	}
}