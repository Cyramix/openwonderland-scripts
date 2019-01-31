/**
 * Project Wonderland
 *
 * Copyright (c) 2004-2009, Sun Microsystems, Inc., All Rights Reserved
 *
 * Redistributions in source code form must reproduce the above
 * copyright and this condition.
 *
 * The contents of this file are subject to the GNU General Public
 * License, Version 2 (the "License"); you may not use this file
 * except in compliance with the License. A copy of the License is
 * available at http://www.opensource.org/licenses/gpl-license.php.
 *
 * Sun designates this particular file as subject to the "Classpath" 
 * exception as provided by Sun in the License file that accompanied 
 * this code.
 */
package org.jdesktop.wonderland.modules.statussample.client;

import com.jme.bounding.BoundingSphere;
import com.jme.math.Vector3f;
import com.jme.scene.Node;
import com.jme.scene.shape.Box;
import org.jdesktop.mtgame.Entity;
import org.jdesktop.wonderland.client.cell.Cell;
import org.jdesktop.wonderland.client.jme.cellrenderer.BasicRenderer;
import org.jdesktop.wonderland.common.cell.CellStatus;

/**
 * An example of a cell renderer
 * 
 * @author jkaplan
 */
public class SampleRenderer extends BasicRenderer {
    private Node node = null;

    public SampleRenderer(Cell cell) {
        super(cell);
    }

    @Override
    public void setStatus(CellStatus status, boolean increasing) {
        logger.warning("[Renderer Status] " + status);
        super.setStatus(status, increasing);
    }

    protected Node createSceneGraph(Entity entity) {

        /* Fetch the basic info about the cell */
        String name = cell.getCellID().toString();

        /* Create the scene graph object and set its wireframe state */
        node = new Node();
        node.attachChild(new Box(name, new Vector3f(), 2, 2, 2));
        node.setModelBound(new BoundingSphere());
        node.updateModelBound();
        node.setName("Cell_"+cell.getCellID()+":"+cell.getName());

        return node;
    }
}
