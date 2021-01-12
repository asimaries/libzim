/*
 * Copyright 2020 Matthieu Gautier <mgautier@kymeria.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU  General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#ifndef OPENZIM_LIBZIM_WRITER_HANDLER_H
#define OPENZIM_LIBZIM_WRITER_HANDLER_H

#include <string>
#include <memory>

#include <zim/writer/item.h>

namespace zim {
namespace writer {

class CreatorData;
class ContentProvider;
class Dirent;

/**
 * DirentHandler is used to add "extra" handling on dirent/item.
 *
 * The main purpose of the handle is to "see" all dirents corresponding to user entries
 * and generate it's own dirent/item.
 *
 * Classical use cases are :
 *  - Generating a index of the item (xapianIndex)
 *  - Generating a listing of the item (all item or "main" entries only)
 *  - Count mimetypes
 *  - ...
 *
 * The workflow is the following:
 * - Start the handler with `start()`.
 * - Pass dirents to handle using `handle()`.
 * - Get the dirent associated to the handler using `getDirent()`.
 *   If a handler has to handle itself, it has to do it itself before (in start/stop, ...)
 *   The handlers will NOT have dirents of other hanlders passed.
 *   TitleListingHandler is a exception, it will receive ALL dirents, including its own.
 * - All dirents are correctly set (redirect resolved, index and mimetype set, ...)
 *   This step can mark dirents as removed (invalid redirect), handler may want to check that.
 * - Stop the handler with `stop()`.
 * - Content of the handler is taken using `getContentProvider`
 *
 *  While it seems that DirentHandler is dynamically (de)activated by user it is not.
 *  This is purelly a internal structur to simplify the internal architecture of the writer.
 */
class DirentHandler {
  public:
    explicit DirentHandler(CreatorData* data);
    virtual ~DirentHandler() = default;

    virtual void start() = 0;
    virtual void stop() = 0;
    Dirent* getUniqueDirent() {
      if (!mp_uniqueDirent) {
        mp_uniqueDirent = getDirent();
      }
      return mp_uniqueDirent;
    }
    virtual std::unique_ptr<ContentProvider> getContentProvider() const = 0;

    /*
     * Handle a dirent/item.
     *
     * item may be nullptr (dirent is a redirect or in special case)
     */
    virtual void handle(Dirent* dirent, const Hints& hints, std::shared_ptr<Item> item) = 0;

  protected:
    virtual Dirent* getDirent() const = 0;
    DirentHandler() = default;

  private:
    Dirent* mp_uniqueDirent {0};
};

}
}

#endif // OPENZIM_LIBZIM_WRITER_HANDLER_H
