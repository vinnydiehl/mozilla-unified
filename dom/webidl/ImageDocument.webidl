/* -*- Mode: IDL; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The origin of this IDL file is:
 * content/html/document/public/nsIImageDocument.idl
 */

interface imgIRequest;

[OverrideBuiltins]
interface ImageDocument : HTMLDocument {
  /* Whether the pref for image resizing has been set. */
  readonly attribute boolean imageResizingEnabled;

  /* Whether the image is overflowing visible area. */
  readonly attribute boolean imageIsOverflowing;

  /* Whether the image has been resized to fit visible area. */
  readonly attribute boolean imageIsResized;

  /* The image request being displayed in the content area */
  [Throws]
  readonly attribute imgIRequest? imageRequest;

  /* Resize the image to fit visible area. */
  void shrinkToFit();

  /* Restore image original size. */
  void restoreImage();

  /* Restore the image, trying to keep a certain pixel in the same position.
   * The coordinate system is that of the shrunken image.
   */
  void restoreImageTo(long x, long y);

  /* A helper method for switching between states.
   * The switching logic is as follows. If the image has been resized
   * restore image original size, otherwise if the image is overflowing
   * current visible area resize the image to fit the area.
   */
  void toggleImageSize();
};
